--[[
  Quest 972: [Dungeon Quest] Wreck of the Hidden Secrets (Beginner)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_972 = fsm:new({
    id = 972,
    name = "[Dungeon Quest] Wreck of the Hidden Secrets (Beginner)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 972: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 972: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 972: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 747,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 972: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 748,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 972: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 749,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 972: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 750,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 972: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 972: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 972: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 972: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 972: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_972