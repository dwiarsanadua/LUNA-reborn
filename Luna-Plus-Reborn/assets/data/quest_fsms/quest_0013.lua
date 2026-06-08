--[[
  Quest 13: Delivering Sad News
  Level Required: 14
  NPC Start: 15, NPC Complete: 22
  Prerequisites: [12]
  Rewards: EXP=1456, Gold=530
]]

local fsm = require('fsm_engine')

local quest_13 = fsm:new({
    id = 13,
    name = "Delivering Sad News",
    level_required = 14,
    npc_start = 15,
    npc_complete = 22,
    prerequisites = {12},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 13: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 13: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 13: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 13: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 13: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 13: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 13: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 13: Rewards given")
                self:give_rewards({
                    exp = 1456,
                    gold = 530,
                    items = {
                        { item_id = 21000007, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_13