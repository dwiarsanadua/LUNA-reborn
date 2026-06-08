--[[
  Quest 973: [Dungeon Quest] The Phantom of the Wreck (Intermediate)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=240995, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_973 = fsm:new({
    id = 973,
    name = "[Dungeon Quest] The Phantom of the Wreck (Intermediate)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 973: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 973: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 973: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 679,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 973: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 680,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 973: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 682,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 973: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 690,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 973: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 973: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 973: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 973: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 973: Rewards given")
                self:give_rewards({
                    exp = 240995,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_973