--[[
  Quest 422: [Quest] Sticky Liquid
  Level Required: 23
  NPC Start: 25, NPC Complete: 25
  Prerequisites: [419]
  Rewards: EXP=14084, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_422 = fsm:new({
    id = 422,
    name = "[Quest] Sticky Liquid",
    level_required = 23,
    npc_start = 25,
    npc_complete = 25,
    prerequisites = {419},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 422: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 422: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 422: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 24,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 422: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 422: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 422: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 422: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 422: Rewards given")
                self:give_rewards({
                    exp = 14084,
                    gold = 3000,
                    items = {
                        { item_id = 308, count = 1 },
                        { item_id = 308, count = 10 },
                        { item_id = 21000009, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_422