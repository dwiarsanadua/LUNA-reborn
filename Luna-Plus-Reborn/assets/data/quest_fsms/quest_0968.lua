--[[
  Quest 968: [Dungeon Quest] Secret Tower (Advanced)
  Level Required: 50
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=240995, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_968 = fsm:new({
    id = 968,
    name = "[Dungeon Quest] Secret Tower (Advanced)",
    level_required = 50,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 968: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 968: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 968: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 486,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 968: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 487,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 968: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 489,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 968: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 492,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 968: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 968: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 968: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 968: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 968: Rewards given")
                self:give_rewards({
                    exp = 240995,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_968