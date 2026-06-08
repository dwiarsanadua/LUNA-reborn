--[[
  Quest 956: [Party] Sad Song 2
  Level Required: 109
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [951]
  Rewards: EXP=17049889, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_956 = fsm:new({
    id = 956,
    name = "[Party] Sad Song 2",
    level_required = 109,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {951},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 956: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 956: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 956: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 327,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 956: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 336,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 956: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 337,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 956: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 338,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 956: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 956: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 956: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 956: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 956: Rewards given")
                self:give_rewards({
                    exp = 17049889,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_956