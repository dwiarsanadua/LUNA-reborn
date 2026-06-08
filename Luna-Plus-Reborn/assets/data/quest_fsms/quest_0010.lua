--[[
  Quest 10: The Kobold Menace
  Level Required: 9
  NPC Start: 14, NPC Complete: 14
  Rewards: EXP=591, Gold=1370
]]

local fsm = require('fsm_engine')

local quest_10 = fsm:new({
    id = 10,
    name = "The Kobold Menace",
    level_required = 9,
    npc_start = 14,
    npc_complete = 14,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 10: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 10: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 10: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 8,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 10: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 31,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 10: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 10: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 10: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 10: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 10: Rewards given")
                self:give_rewards({
                    exp = 591,
                    gold = 1370,
                    items = {
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_10