--[[
  Quest 965: [Dungeon Quest]Evil Towers(Intermediate)
  Level Required: 40
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=95297, Gold=6609
]]

local fsm = require('fsm_engine')

local quest_965 = fsm:new({
    id = 965,
    name = "[Dungeon Quest]Evil Towers(Intermediate)",
    level_required = 40,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 965: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 965: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 965: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 464,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 965: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 468,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 965: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 470,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 965: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 472,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 965: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 965: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 965: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 965: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 965: Rewards given")
                self:give_rewards({
                    exp = 95297,
                    gold = 6609,
                })
            end,
            transitions = {},
        },
    },
})

return quest_965