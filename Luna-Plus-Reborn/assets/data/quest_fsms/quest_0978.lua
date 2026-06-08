--[[
  Quest 978: [Dungeon Quest] Wreck of the Hidden Secrets (Advanced)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_978 = fsm:new({
    id = 978,
    name = "[Dungeon Quest] Wreck of the Hidden Secrets (Advanced)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 978: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 978: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 978: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 755,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 978: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 756,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 978: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 757,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 978: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 758,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 978: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 978: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 978: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 978: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 978: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 12128,
                })
            end,
            transitions = {},
        },
    },
})

return quest_978