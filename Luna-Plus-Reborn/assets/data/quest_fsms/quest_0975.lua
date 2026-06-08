--[[
  Quest 975: [Dungeon Quest] Hidden Secrets of the Wreck (Intermediate)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_975 = fsm:new({
    id = 975,
    name = "[Dungeon Quest] Hidden Secrets of the Wreck (Intermediate)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 975: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 975: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 975: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 751,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 975: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 752,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 975: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 753,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 975: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 754,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 975: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 975: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 975: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 975: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 975: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_975