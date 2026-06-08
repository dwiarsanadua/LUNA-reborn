--[[
  Quest 966: [Dungeon Quest] The Magic Secret of the Tower (Intermediate)
  Level Required: 40
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=75234, Gold=6609
]]

local fsm = require('fsm_engine')

local quest_966 = fsm:new({
    id = 966,
    name = "[Dungeon Quest] The Magic Secret of the Tower (Intermediate)",
    level_required = 40,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 966: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 966: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 966: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 477,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 966: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 478,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 966: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 966: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 966: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 966: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 966: Rewards given")
                self:give_rewards({
                    exp = 75234,
                    gold = 6609,
                    items = {
                        { item_id = 21000008, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_966