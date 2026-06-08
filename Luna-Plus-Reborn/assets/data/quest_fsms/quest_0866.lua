--[[
  Quest 866: [Party]True hunter
  Level Required: 91
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=4930862, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_866 = fsm:new({
    id = 866,
    name = "[Party]True hunter",
    level_required = 91,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 866: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 866: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 866: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 866: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 866: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 866: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 866: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 866: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 866: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 866: Rewards given")
                self:give_rewards({
                    exp = 4930862,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_866