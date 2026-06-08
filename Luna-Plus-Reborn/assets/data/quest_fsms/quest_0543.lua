--[[
  Quest 543: [Repeat][Quest] Fair Wings
  Level Required: 150
  NPC Start: 52, NPC Complete: 99
  Prerequisites: [542]
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_543 = fsm:new({
    id = 543,
    name = "[Repeat][Quest] Fair Wings",
    level_required = 150,
    npc_start = 52,
    npc_complete = 99,
    prerequisites = {542},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 543: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 543: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 543: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 543: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 543: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 543: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 543: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 543: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 543: Rewards given")
                self:give_rewards({
                    exp = 1000000,
                    items = {
                        { item_id = 339, count = 1 },
                        { item_id = 30000021, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_543