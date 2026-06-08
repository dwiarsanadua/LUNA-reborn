--[[
  Quest 120: Lab Report
  Level Required: 21
  NPC Start: 7, NPC Complete: 24
  Prerequisites: [119]
  Rewards: EXP=4481, Gold=915
]]

local fsm = require('fsm_engine')

local quest_120 = fsm:new({
    id = 120,
    name = "Lab Report",
    level_required = 21,
    npc_start = 7,
    npc_complete = 24,
    prerequisites = {119},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 120: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 120: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 120: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 120: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 120: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 120: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 120: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 120: Rewards given")
                self:give_rewards({
                    exp = 4481,
                    gold = 915,
                    items = {
                        { item_id = 57, count = 1 },
                        { item_id = 57, count = 1 },
                        { item_id = 21000009, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_120