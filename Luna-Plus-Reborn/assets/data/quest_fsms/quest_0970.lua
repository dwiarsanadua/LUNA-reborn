--[[
  Quest 970: [Dungeon Quest] The Phantom of the Wreck (Beginner)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=240995, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_970 = fsm:new({
    id = 970,
    name = "[Dungeon Quest] The Phantom of the Wreck (Beginner)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 970: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 970: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 970: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 675,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 970: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 676,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 970: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 678,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 970: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 687,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 970: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 970: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 970: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 970: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 970: Rewards given")
                self:give_rewards({
                    exp = 240995,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_970