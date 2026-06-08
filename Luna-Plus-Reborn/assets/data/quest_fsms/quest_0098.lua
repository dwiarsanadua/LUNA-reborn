--[[
  Quest 98: Alfred's Letter
  Level Required: 19
  NPC Start: 7, NPC Complete: 15
  Rewards: EXP=3441, Gold=550
]]

local fsm = require('fsm_engine')

local quest_98 = fsm:new({
    id = 98,
    name = "Alfred's Letter",
    level_required = 19,
    npc_start = 7,
    npc_complete = 15,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 98: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 98: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 98: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 98: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 98: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 98: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 98: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 98: Rewards given")
                self:give_rewards({
                    exp = 3441,
                    gold = 550,
                    items = {
                        { item_id = 41, count = 1 },
                        { item_id = 41, count = 1 },
                        { item_id = 21000009, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_98