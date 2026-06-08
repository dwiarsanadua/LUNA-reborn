--[[
  Quest 3: Preparations For Your Journey
  Level Required: 1
  NPC Start: 2, NPC Complete: 9
  Prerequisites: [2]
  Rewards: EXP=4, Gold=530
]]

local fsm = require('fsm_engine')

local quest_3 = fsm:new({
    id = 3,
    name = "Preparations For Your Journey",
    level_required = 1,
    npc_start = 2,
    npc_complete = 9,
    prerequisites = {2},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 3: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 3: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 3: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 3: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 3: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 3: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 3: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 3: Rewards given")
                self:give_rewards({
                    exp = 4,
                    gold = 530,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_3