--[[
  Quest 2017: [Daily] Ogre Warrior
  Level Required: 67
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=460160
]]

local fsm = require('fsm_engine')

local quest_2017 = fsm:new({
    id = 2017,
    name = "[Daily] Ogre Warrior",
    level_required = 67,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2017: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2017: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2017: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2017: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2017: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2017: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2017: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2017: Rewards given")
                self:give_rewards({
                    exp = 460160,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2017